<?php

/**
 * @file
 * Form handling for the administration of Simon Fraser University.
 */

/**
 * Defines the Simon Fraser University admin form.
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

  $form['root_collection'] = array(
    '#type' => 'textfield',
    '#title' => t('Root Collection PID for ingest'),
    '#default_value' => variable_get('islandora_pydio_bridge_repository_pid', 'islandora:root'),
    '#description' => t('The PID of the Root Collection Object'),
    '#required' => TRUE,
  );

  $form['pydio_path'] = array(
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

  // Set a specific callback for the submit button.
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save Configuration'),
  );

  return $form;
}

/**
 * Form validation function.
 */
function islandora_pydio_bridge_admin_form_validate(&$form, &$form_state) {

  $object = islandora_object_load($form_state['values']['root_collection']);
  if (!$object) {
    form_set_error('root_collection', t('%pid does not exist. Enter a valid PID.', array(
      '%pid' => $form_state['values']['root_collection'],
    )));
  }
}

/**
 * Form submission function.
 */
function islandora_pydio_bridge_admin_form_submit(&$form, &$form_state) {
  variable_set('islandora_pydio_bridge_pid_namespace', $form_state['values']['islandora_pydio_bridge_pid_namespace']);
  variable_set('islandora_pydio_bridge_pydio_path', $form_state['values']['pydio_path']);
  variable_set('islandora_pydio_bridge_repository_pid', $form_state['values']['root_collection']);
  drupal_set_message(t('Islandora Pydio Bridge settings saved.'), 'status');
}
