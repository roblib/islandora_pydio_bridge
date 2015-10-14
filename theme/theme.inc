<?php

/**
 * @file
 * Theme preprocessor and helper functions
 */
function islandora_pydio_bridge_versions($object) {
  return theme('islandora_pydio_bridge_versions', array('object' => $object));
}

function islandora_pydio_bridge_get_ds_versions($object, $dsid) {
  $ds = $object[$dsid];
  // We may have more dsids to work with in the future.
  switch ($dsid) {
    case 'DDI' :
      $label = 'Metadata';
      break;
    default :
      $label = 'Data';
  }

  if (!empty($ds)) {
    $ds_table = islandora_datastream_version_table($ds);
  }
  else {
    $ds_table = t('Tried to get versions for @dsid datastream but no datastream found.', array('@dsid' => $dsid));
  }
  $versions = t('Versions');

  return "<h2 class='islandora-pydio-bridge-event-output'>$label $versions</h2>"
      . "<div class='islandora-pydio-bridge-version-output-table'>" . $ds_table . '</div></h2>';
}

/**
 * Implements hook_preprocess_theme().
 */
function template_preprocess_islandora_pydio_bridge_versions(array &$variables) {
  module_load_include('inc', 'islandora', 'includes/datastream.version');
  module_load_include('inc', 'islandora_pydio_bridge', 'includes/utilities');
  $object = $variables['object'];
  $variables['obj_dsv_table'] = islandora_pydio_bridge_get_ds_versions($object, 'OBJ');
  $variables['ddi_dsv_table'] = islandora_pydio_bridge_get_ds_versions($object, 'DDI');
  $variables['lifecycle_events_table'] = islandora_pydio_bridge_events_table($object['DDI']);
  $variables['object_status_table'] = islandora_pydio_bridge_status_table($object['DDI']);
  drupal_set_title(t('VERSIONS OF -- @label', array('@label' => $object->label)));
}

/**
 * Function that returns markup for the DDI lifecycle events.
 * 
 */
function islandora_pydio_bridge_events_table($datastream) {
  $parent = $datastream->parent;

  $header = array();
  $header[] = array('data' => t('Lifecycle Event Date'));
  $header[] = array('data' => t('Lifecycle Event ID'));
  $header[] = array('data' => t('Lifecycle Event Type'));
  $header[] = array('data' => t('Lifecycle Event Description'));
  $xml = new DOMDocument();
  $test = $xml->loadXML($datastream->content);
  $query = "//a:Archive/r:LifecycleInformation";
  $xpath = new DOMXPath($xml);
  $xpath->registerNamespace('r', 'ddi:reusable:3_2');
  $xpath->registerNamespace('a', 'ddi:archive:3_2');
  $results = $xpath->query($query, $xml);
  $rows = array();

  foreach ($results as $node) {
    $row = array();
    $e_date = $node->getElementsByTagName('SimpleDate')->item(0)->nodeValue;
    $e_id = $node->getElementsByTagName('ID')->item(0)->nodeValue;
    $e_type = $node->getElementsByTagName('EventType')->item(0)->nodeValue;
    $e_description = $node->getElementsByTagName('Content')->item(0)->nodeValue;


    $row[] = array(
      'class' => 'lifecycle-date',
      'data' => $e_date,
    );
    $row[] = array(
      'class' => 'lifecycle-id',
      'data' => $e_id,
    );
    $row[] = array(
      'class' => 'lifecycle-type',
      'data' => $e_type,
    );
    $row[] = array(
      'class' => 'lifecycle-description',
      'data' => $e_description,
    );

    $rows[] = $row;
  }

  return theme('table', array('header' => $header, 'rows' => $rows));
}

/**
 * Function that returns markup for the DDI lifecycle events.
 * 
 */
function islandora_pydio_bridge_status_table($datastream) {
  module_load_include('inc', 'islandora_datacite_doi', 'includes/datacite_doi');
  $parent = $datastream->parent;

  $header = array();
  $header[] = array('data' => t('Created Date'));
  $header[] = array('data' => t('Last Modified Date'));
  $header[] = array('data' => t('Datacite DOI'));
  $header[] = array('data' => t('Status'));
  $doi = islandora_pydio_bridge_get_ddi_identifier($datastream->content);
  $status = islandora_pydio_bridge_get_ddi_status($datastream->content);
  $rows = array();
  $row[] = array(
    'class' => 'object-created-date',
    'data' => $parent->createdDate,
  );
  $row[] = array(
    'class' => 'object-modified-date',
    'data' => $parent->lastModifiedDate,
  );
  $row[] = array(
    'class' => 'ddi-doi',
    'data' => $doi,
  );
  $row[] = array(
    'class' => 'ddi-status',
    'data' => $status,
  );

  $rows[] = $row;


  return theme('table', array('header' => $header, 'rows' => $rows));
}
