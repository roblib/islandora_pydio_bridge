<?php

/**
 * @file
 * Form handling for the administration of Islandora Pydio Bridge.
 */

/**
 * Defines the Islandora Pydio Bridge admin form.
 *
 * @param array $form
 *   An array representing a Drupal form.
 * @param array $form_state
 *   The Drupal form state.
 *
 * @return array
 *   An array representing the Simon Fraser University admin form.
 */
function islandora_pydio_bridge_admin_form($form, &$form_state) {
  form_load_include($form_state, 'inc', 'islandora_pydio_bridge', 'includes/admin.form');

  $form = array();

  $form['islandora_pydio_bridge_repository_pid'] = array(
    '#type' => 'textfield',
    '#title' => t('Root Collection PID for ingest'),
    '#default_value' => variable_get('islandora_pydio_bridge_repository_pid', 'islandora:pydio_data_top'),
    '#description' => t('The PID of the Root Collection Object'),
    '#required' => TRUE,
  );
  $form['islandora_pydio_bridge_pydio_path '] = array(
    '#type' => 'textfield',
    '#title' => t('Pydio path'),
    '#default_value' => variable_get('islandora_pydio_bridge_pydio_path', 'pydio/'),
    '#description' => t('The pydio server url and path'),
    '#required' => TRUE,
  );

  $form['islandora_pydio_bridge_pid_namespace'] = array(
    '#type' => 'textfield',
    '#title' => t('Pydio Pid Namespace'),
    '#default_value' => variable_get('islandora_pydio_bridge_pid_namespace', 'pydio'),
    '#description' => t('The pid namespace to use for items exported from Pydio'),
    '#required' => TRUE,
  );
  
  $form['islandora_pydio_bridge_agency'] = array(
    '#type' => 'textfield',
    '#title' => t('Agency Name'),
    '#default_value' => variable_get('islandora_pydio_bridge_agency', 'ddi agency'),
    '#description' => t('The agency name to use in DDI LifecycleEvents'),
    '#required' => TRUE,
  );

  return system_settings_form($form);
}

/**
 * Form validation function.
 */
function islandora_pydio_bridge_admin_form_validate(&$form, &$form_state) {

  $object = islandora_object_load($form_state['values']['islandora_pydio_bridge_repository_pid']);
  if (!$object) {
    form_set_error('root_collection', t('%pid does not exist. Enter a valid PID.', array(
      '%pid' => $form_state['values']['root_collection'],
    )));
  }
}

