<?php

/**
 * @file
 * Utility functions
 */

/**
 * Scan the file using clamav virus scanner.
 *
 * We have a soft dependancy on clamav as it is currently only alpha and some
 * users may not want alpha modules in their stack.
 *
 * @param string $uri
 *   The path to the file.
 *
 * @return int
 *   The results of the scan.
 */
function islandora_pydio_bridge_scan($uri) {
  if (file_uri_scheme($uri) == "zip") {
    // Copy the file so clamav can scan it.
    $tmpfile = tempnam(sys_get_temp_dir(), 'pydio');
    copy($uri, $tmpfile);
    $result = function_exists("clamav_scan_file") ? clamav_scan_file(drupal_realpath($tmpfile)) :
        ISLANDORA_SCANRESULT_UNCHECKED;
    unlink($tmpfile);
  }
  else {
    $result = function_exists("clamav_scan_file") ? clamav_scan_file(drupal_realpath($uri)) :
        ISLANDORA_SCANRESULT_UNCHECKED;
  }
  return $result;
}

/**
 * Get the clamav settings..
 *
 * @return array|null
 *   An array containing the clamav path or the host port for daemon mode
 */
function islandora_pydio_bridge_get_clamav_settings() {
  $mode = variable_get('clamav_mode', CLAMAV_DEFAULT_MODE);
  if (!empty($mode) && $mode == CLAMAV_USE_DAEMON) {
    $settings = array(
      'host' => variable_get('clamav_daemon_host', CLAMAV_DEFAULT_HOST),
      'port' => variable_get('clamav_daemon_port', CLAMAV_DEFAULT_PORT),
    );
  }
  else {
    $settings = variable_get('clamav_executable_path', CLAMAV_DEFAULT_PATH);
  }
  return $settings;
}

/**
 * Create a string to use in the audit trail.
 *
 * @param $result
 *   the result of a clamav scan
 *
 * @return string
 *   A string formatted as DCMI DCSV for use in the fedora objects audit trail
 */
function islandora_pydio_bridge_create_log($result) {
  $settings = islandora_pydio_bridge_get_clamav_settings();
  $clamav_version = function_exists("clamav_get_version") && !empty($settings) ?
      clamav_get_version($settings) :
      'no antivirus installed';
  $return_value = "event type: virus check\n";
  if ($clamav_version == "no antivirus installed" || $result == ISLANDORA_SCANRESULT_UNCHECKED) {
    return $return_value . "eventDetail = Not scanned\n eventOutcome = No op";
  }
  elseif ($result == 0) {
    return $return_value . "eventDetail = program = \"ClamAv -clamscan\";version = \"$clamav_version\" \n eventOutcome = pass";
  }
  else {
    return $return_value . "eventDetail = program = \"ClamAv -clamscan\";version = \"$clamav_version\" \n eventOutcome = fail, OBJ datastream was blocked";
  }
}

/**
 * Mint a doi and add it to an object.  If successfull also add a DDI lifecycle event stating we have added a DOI.
 *
 * @param AbstractObject $object
 *   The object (or its pid) that the xacml should be added to
 * @param string $restriction
 *  The type of restriction (Public, Archive or Embargoed).
 *  Public - a DOI is created.
 *  Embargoed -  a DOI is created.
 *  Archive - no DOI created.
 */
function islandora_pydio_bridge_ingest_create_doi($object, $restriction) {
  // Don't create a DOI if we just want to archive the data
  if ($restriction == 'Archive') {
    return;
  }
  $result = islandora_datacite_doi_mint_doi($object, 'DDI', 'DDI');
  if ($result < 0) {
    drupal_set_message(t('DOI error for @object, check watchdog logs for more information.', array('@object' => $object->id)), 'warning');
    return;
  }
  islandora_pydio_bridge_add_ddi_event($object['DDI'], 'Other', 'minted a DOI and updated the DDI with the DOI');
}

/**
 * Set embargo of the object.
 * 
 * Currently we leave the DDI unembargoed
 *
 * @param array $objects
 *   The object or the pid of the object.
 * @param string $end
 *   The expire date (yyyy-mm-dd).
 */
function islandora_pydio_bridge_ingest_set_embargo($objects, $end = "indefinite") {
  module_load_include('inc', 'islandora_scholar_embargo');
  foreach ($objects as $object) {
    $ds_array = array();
    foreach ($object as $datastream) {
      // Embargo all datastreams except DDI
      if ($datastream->id != 'DDI') {
        $ds_array[] = $datastream->id;
      }
    }
    islandora_scholar_embargo_set_embargo($object, $ds_array, $end);
  }
}

/**
 * Add the xacml to an object.
 *
 * @param AbstractObject $object
 *   The object (or its pid) that the xacml should be added to
 * @param string $restriction
 *  The type of restriction (Public, Archive or Embargoed).
 *  Public - Public users can access description and download/view the dataset 
 *  Embargoed - Public users can access the description, but need to contact researcher for dataset view/download 
 *  Archive - Only Researcher/Admin can view/access
 */
function islandora_pydio_bridge_ingest_set_xacml($object, $restriction) {
  switch ($restriction) {
    case "Public" :
      $xacml = new IslandoraXacml($object);
      $xacml->managementRule->addUser(array($object->owner, 'admin'));
      $xacml->writeBackToFedora();
      break;
    case "Archive" :
      $xacml = new IslandoraXacml($object);
      $xacml->viewingRule->addUser(array($object->owner, 'admin'));
      $xacml->managementRule->addUser(array($object->owner, 'admin'));
      $xacml->writeBackToFedora();
      break;
    // Scholar embargo module creates the xacml for Embargoed case
  }
}


/**
 * Parse the DDI looking for the a:AccessCondition elements Content value.
 * 
 * @param string $ddi
 * @return string
 *   Returns the DDI accesscondition/content value or NULL
 */
function islandora_pydio_bridge_get_ddi_status($ddi) {
  $xml = new DOMDocument();
  $test = $xml->loadXML($ddi);
  $query = "//a:AccessConditions/r:Content";
  $xpath = new DOMXPath($xml);
  $xpath->registerNamespace('r', 'ddi:reusable:3_2');
  $xpath->registerNamespace('a', 'ddi:archive:3_2');
  $results = $xpath->query($query, $xml);
  if ($results->length > 0) {
    return $results->item(0)->nodeValue;
  }
  return NULL;
}

/**
 * Parse the DDI looking for the r:IdentifierContent value.
 * 
 * @param string $ddi
 * @return string
 *   Returns the IdentifierContent value or NULL
 */
function islandora_pydio_bridge_get_ddi_identifier($ddi) {
  $xml = new DOMDocument();
  $test = $xml->loadXML($ddi);
  $query = "//r:Citation/r:InternationalIdentifier[r:ManagingAgency/text() = 'Datacite']/r:IdentifierContent";
  $xpath = new DOMXPath($xml);
  $xpath->registerNamespace('r', 'ddi:reusable:3_2');
  $results = $xpath->query($query, $xml);
  if ($results->length > 0) {
    return $results->item(0)->nodeValue;
  }
  return NULL;
}

/**
 * Add a lifecycle event to the DDI xml.
 * 
 * @param Object $datastream
 *   An Islandora datastream
 * @param string $type
 *   The type of event should adhere to the DDI LifecycleEvent type vocabulary.
 * @param string $content
 *   The event description
 * @param bool $update
 *   If true update the datastream content with the xml if false return the xml.
 * 
 * @return null/string
 *   will return the xml if $update = false otherwise will return null. 
 */
function islandora_pydio_bridge_add_ddi_event($datastream, $type, $content, $update = TRUE) {
  $agency = variable_get('islandora_pydio_bridge_agency', 'ddi agency');
  // Colon is not valid in a ddi ID
  $id = uniqid(str_replace(':', '_', $datastream->parent->id) . '-');
  $event_values = array('agency' => $agency, 'id' => $id,
    'version' => '1', 'type' => $type, 'content' => $content);
  $xml = new DOMDocument();
  $xml->loadXML($datastream->content);
  $xml = DataciteDoi::addDDIEvent($xml, $event_values);
  if($update){
    $datastream->content = $xml->saveXML();
  } else {
    return $xml->saveXML();
  }
}

