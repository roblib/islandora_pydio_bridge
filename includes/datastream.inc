<?php
/**
 * @file
 * This file contains function related to creating and manipulating datastreams
 */

/**
 * Creates a OBJ Datastream.
 *
 * @param array $form
 *   The form.
 * @param string $form_state
 *   The form state.
 * @param object $new_object
 *   The new Object.
 *
 * @return bool
 *   returns TRUE if the OBJ is added, FALSE otherwise
 */
function islandora_pydio_bridge_ingest_create_obj_datastream(&$form, &$form_state, $new_object, $file_info) {
  module_load_include('inc', 'islandora', 'includes/mime_detect');
  module_load_include('inc', 'clamav', 'clamav');
  $mime_detect = new MimeDetect();
  //$obj_mimetype = $mime_detect->getMimetype($file_info->file_name);
  $obj_mimetype = file_get_mimetype($file_info->file_name);
  $result = islandora_pydio_bridge_scan($file_info->uri);
  // Some sites may not have clamav installed as it is alpha only at this point.
  if ($result == ISLANDORA_SCANRESULT_CLEAN || $result == ISLANDORA_SCANRESULT_UNCHECKED) {
    // Create the OBJ datastream.
    $obj_ds = $new_object->constructDatastream('OBJ', 'M');
    $obj_ds->label = $file_info->file_name;
    $obj_ds->mimeType = $obj_mimetype;
    $obj_ds->logMessage = islandora_pydio_bridge_create_log($result);
    $obj_ds->setContentFromFile($file_info->uri, FALSE);
    $new_object->logMessage = islandora_pydio_bridge_create_log($result);
    $new_object->ingestDatastream($obj_ds);
  }
  else {
    drupal_set_message(t("Virus scan failed, cannot process %file_name.  OBJ datastream blocked", array('%file_name' => $file_info->file_name)), 'error');
    $new_object->logMessage = islandora_pydio_bridge_create_log($result);
    return FALSE;
  }
  return TRUE;
}

/**
 * Updates a OBJ Datastream.
 *
 * @param array $form
 *   The form.
 * @param string $form_state
 *   The form state.
 * @param object $file_info
 *   The file info.
 * @param object $update_object
 *   The pid of the object to update.
 *
 * @return bool
 */
function islandora_pydio_bridge_update_obj_datastream(&$form, &$form_state, $file_info, $update_object) {
  $pid = $update_object['pid'];
  $object = islandora_object_load($pid);
  // an object may be missing the OBJ if it was blocked by a previous antivirus check so we may have to add it.
  if (empty($object['OBJ'])) {
    return islandora_pydio_bridge_ingest_create_obj_datastream($form, $form_state, $object, $file_info);
  }
  $result = islandora_pydio_bridge_scan($file_info->uri);
  if ($result == 0 || $result == -1) {
    $object['OBJ']->logMessage = islandora_pydio_bridge_create_log($result);
    $object['OBJ']->setContentFromFile($file_info->uri, FALSE);
  }
  else {
    drupal_set_message(t("Virus scan failed, cannot process %file_name.  OBJ datastream blocked", array('%file_name' => $file_info->file_name)), 'error');
    $object->logMessage = islandora_pydio_bridge_create_log($result);
    return FALSE;
  }
  return TRUE;
}
/**
 * Creates a COLLECTION_POLICY Datastream.
 *
 * @param object $new_object
 *   The new Object.
 * @param string $parent_collection 
 *   The parent collection
 */
function islandora_pydio_bridge_ingest_create_policy_datastream($new_object, $parent_collection) {
  $collection = islandora_object_load($parent_collection);
  $policy_xml = $collection['COLLECTION_POLICY']->content;
  $policy = new CollectionPolicy($policy_xml);
  // Create the OBJ datastream.
  $obj_ds = $new_object->constructDatastream('COLLECTION_POLICY', 'M');
  $obj_ds->label = 'COLLECTION_POLICY';
  // Currently Islandora creates Collectioin policies as text/xml so we do the same
  // we may want to revisit this.
  $obj_ds->mimeType = 'text/xml';
  $obj_ds->setContentFromString($policy->getXML());
  $new_object->ingestDatastream($obj_ds);
}

/**
 * Creates a DDI Datastream.
 *
 * @param array $form
 *   The form.
 * @param string $form_state
 *   The form state.
 * @param object $new_object
 *   The new Object.
 */
function islandora_pydio_bridge_ingest_create_ddi_datastream(&$form, &$form_state, $new_object) {

  // Make copies of the form and form_state.
  $ddi_form = $form;
  $ddi_form_state = $form_state;

  // All of the form values that are not part of the form builder form will be
  // removed for processing.
  unset($ddi_form['restrict_options']);
  unset($ddi_form['embargo_duration']);
  unset($ddi_form['embargo_date']);
  unset($ddi_form['xacml']);
  unset($ddi_form_state['values']['restrict_options']);
  unset($ddi_form_state['values']['embargo_duration']);
  unset($ddi_form_state['values']['embargo_date']);
  unset($ddi_form_state['values']['xacml']);

  // Create the DDI datastream.

  $xml_form = new XMLForm($ddi_form_state);
  $ddi_doc = $xml_form->submit($ddi_form, $ddi_form_state);
  $ddi_ds = $new_object->constructDatastream('DDI', 'M');
  $ddi_ds->label = 'DDI Record';
  $ddi_ds->mimeType = 'application/xml';
  $ddi_ds->setContentFromString($ddi_doc->document->saveXML());
  islandora_pydio_bridge_add_ddi_event($ddi_ds, 'Deposit', 'initial deposit of object');
  $new_object->ingestDatastream($ddi_ds);
}


