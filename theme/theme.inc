<?php

function islandora_pydio_bridge_versions($object) {
  return theme('islandora_pydio_bridge_versions', array('object' => $object));
}

function islandora_pydio_bridge_get_ds_versions($object, $dsid){
  $ds = $object[$dsid];
  
  if (!empty($ds)) {
    $ds_table = islandora_datastream_version_table($ds);
  } else {
    $ds_table =  t('Tried to get versions for @dsid datastream but no datastream found.',
        array('@dsid' => $dsid));
  }
  $versions = t('Versions');
  //TODO make more better when we get some info on layout.
  return "<div class='islandora-pydio-bridge-event-output'>$dsid $versions</br>" . $ds_table . '</div>';
}

/**
 * Implements hook_preprocess_theme().
 */
function template_preprocess_islandora_pydio_bridge_versions(array &$variables) {
  module_load_include('inc', 'islandora', 'includes/datastream.version');
  $object = $variables['object'];
  $ds = $object['OBJ'];
  $variables['obj_dsv_table'] = islandora_pydio_bridge_get_ds_versions($object, 'OBJ');
  $variables['ddi_dsv_table'] = islandora_pydio_bridge_get_ds_versions($object, 'DDI');
  drupal_set_title(t('VERSIONS OF -- @label', array('@label' => $object->label)));
}


