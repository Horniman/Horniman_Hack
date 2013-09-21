<?php

/**
 * @file
 * The main core functions used by all pages
 *
 */

/**
 * Bootstrap the system
 */
function coral_bootstrap() {
  chdir(dirname(realpath(__FILE__)));
  global $config, $request, $db, $user;

  // Enable display of all errors - disable in production
  ini_set('error_reporting', E_ALL);
  ini_set('display_errors', 'On');

  // Load the configuration
  $config = parse_ini_file('../config.ini');
  leak_debug('config', $config);

  // Connect to the database
  $db = new PDO("mysql:host={$config['db_host']};dbname={$config['db_name']}",
    $config['db_user'], $config['db_pass']);

  // TODO: Load the current user
  session_start();
  $user = leak_active_user();
  leak_debug('user', $user);

  require 'system/request.inc';
  $request = leak_parse_request();
  leak_debug('request', $request);

}


/**
 * Execute the current request
 */
function coral_execute() {
  global $request, $user, $config;

  require_once "controllers/{$request['controller']}/{$request['controller']}.inc";
  $access_callback = "{$request['controller']}_access";
  if (function_exists($access_callback) && $access_callback($user)) {
    $callback = "{$request['controller']}_{$request['op']}_execute";
    if (function_exists($callback)) {
      $vars = $callback();

      // Setup some common page variables
      $vars['page_title'] = 'CORAL';
      $vars['messages'] = leak_get_messages();
    }
    else {
      print_r($request);
      $vars = leak_error();
    }
  }
  else {
    $vars = leak_access_denied();
  }
  $vars['logged_in'] = !empty($user['id']);

  // If debugging is enabled add to page variables
  if ($config['debug']) {
    $vars['debug'] = leak_get_debug_output();
  }

  // Format output depending on requested format
  switch ($request['format']) {
    case 'html':
      echo leak_template('html', $vars);
      break;
    case 'json':
      header('Content-Type: application/json');
      if (is_array($vars['content'])) {
        echo json_encode($vars['content']);
      }
      else {
        echo json_encode($vars);
      }
      break;
  }
}

/**
 * Add data to the debug output
 */
function leak_debug($name, $var) {
  global $debug;
  $debug[$name] = $var;
}

/**
 * Function to get all debug output
 */
function leak_get_debug_output() {
  global $debug;
  $vars = array('debug' => print_r($debug, TRUE));
  return leak_template('debug', $vars, 'templates/system');
}

/**
 * Convert some variables to rendered output
 */
function leak_template($template_name, $variables, $folder = 'templates') {
  extract($variables, EXTR_SKIP);
  // Start output buffering
  ob_start();

  // Include the template file
  include $folder . '/' . $template_name . '.tpl.php';
  // End buffering and return its contents
  return ob_get_clean();
}


function leak_active_user() {
  if (!empty($_SESSION['user'])) {
    $user = $_SESSION['user'];
  }
  else {
    $user = array(
      'id' => 0,
    );
  }
  return $user;
}


function leak_access_denied() {
  $output = array('page_title' => 'Access Denied');
  global $user;
  if (empty($user['id'])) {
    $output['content'] = leak_template('login', array(), 'templates/system');
  }
  else {
    $output['content'] = 'You do not have access to this page';
  }
  return $output;
}

function leak_goto($path = '') {
  if ($path == '') {
    header('Location: /');
  }
  else {
    header('Location: ' . $path);
  }
}


/** Allow messages to be set that are collected and displayed
together in the page template **/

/**
 * Set a message, type should be success or alert
 */
function leak_set_message($message, $type = 'alert') {
  global $messages;
  $messages[] = array('content' => $message, 'type' => $type);
}

function leak_get_messages() {
  $output = '';
  global $messages;
  if (!empty($messages)) {
    foreach ($messages as $message) {
      $output .= leak_template('message',
        array('message' => $message['content'],
          'type' => $message['type']), 'templates/system');
    }
  }
  return leak_template('messages', array('content' => $output),
    'templates/system');
}






