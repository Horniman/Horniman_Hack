<?php

function observations_access($user) {
  return !empty($user['id']);
}

function observations_list_execute() {
  global $db;
  $vars = array(
    'observations' => array(),
  );
  $output = coral_template('list', $vars, 'controllers/observations');
  return array('content' => $output);
}

function observations_add_execute() {
  $observation = observations_new_observation();
  $form = coral_template('edit_form', $observation, 'controllers/observations');
  return array('content' => $form);
}

function observations_new_observation() {
  return array(
    'subject' => '',
    'text' => '',
    'time' => $_SERVER['REQUEST_TIME'],
    'reminder' => $_SERVER['REQUEST_TIME'],
  );
}
