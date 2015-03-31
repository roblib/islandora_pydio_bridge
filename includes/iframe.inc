<?php

/**
 * @file
 * Iframe page callback (and associated, if there comes to be any?).
 */

/**
 * Page callback; render iframe containing Pydio.
 */
function islandora_pydio_bridge_frame() {
  $output = array();
  $module_path = drupal_get_path('module', 'islandora_pydio_bridge');
  drupal_add_js("$module_path/js/islandora_pydio_bridge_frame.js");
  drupal_add_css("$module_path/css/islandora_pydio_bridge.theme.css");

  $sfu_path = variable_get('islandora_pydio_bridge_pydio_path', 'pydio');
  $output['Islandora Pydio Bridge'] = array(
    '#markup' => "<iframe id=\"pydio_frame\" src=\"$sfu_path\"></iframe>",
  );
  return $output;
}
