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
  coral_debug('config', $config);

  // Connect to the database
  $db = new PDO("mysql:host={$config['db_host']};dbname={$config['db_name']}",
    $config['db_user'], $config['db_pass']);

  // TODO: Load the current user
  session_start();
  $user = coral_active_user();
  coral_debug('user', $user);

  require 'system/request.inc';
  $request = coral_parse_request();
  coral_debug('request', $request);

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
      $vars['messages'] = coral_get_messages();
    }
    else {
      print_r($request);
      $vars = coral_error();
    }
  }
  else {
    $vars = coral_access_denied();
  }
  $vars['logged_in'] = !empty($user['id']);
  if (!isset($vars['page_title'])) {
    $vars['page_title'] = 'CORAL';
  }

  // If debugging is enabled add to page variables
  if ($config['debug']) {
    $vars['debug'] = coral_get_debug_output();
  }

  // Format output depending on requested format
  switch ($request['format']) {
    case 'html':
      echo coral_template('html', $vars);
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
 * Function to calculate a path
 */
function coral_path($path = '/') {
  global $config;
  if (isset($config['base_path'])) {
    return $config['base_path'] . $path;
  }
  if (substr($path, 1, 1) != '/') {
    return '/' . $path;
  }
  return $path;
}

/**
 * Function to generate an internal link
 */
function l($title, $path) {
  return '<a href="' . coral_path($path) . '">' . $title . '</a>';
}


/**
 * Add data to the debug output
 */
function coral_debug($name, $var) {
  global $debug;
  $debug[$name] = $var;
}

/**
 * Function to get all debug output
 */
function coral_get_debug_output() {
  global $debug;
  $vars = array('debug' => print_r($debug, TRUE));
  return coral_template('debug', $vars, 'templates/system');
}

/**
 * Convert some variables to rendered output
 */
function coral_template($template_name, $variables, $folder = 'templates') {
  extract($variables, EXTR_SKIP);
  // Start output buffering
  ob_start();

  // Include the template file
  include $folder . '/' . $template_name . '.tpl.php';
  // End buffering and return its contents
  return ob_get_clean();
}


function coral_active_user() {
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


function coral_access_denied() {
  global $user;
  if (empty($user['id'])) {
    coral_set_message('You do not have access to this page', 'alert');
    $output['content'] = coral_template('login', array(), 'templates/system');
  }
  else {
    $output['content'] = 'You do not have access to this page';
  }
  return $output;
}

function coral_goto($path = '') {
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
function coral_set_message($message, $type = 'alert') {
  global $messages;
  $messages[] = array('content' => $message, 'type' => $type);
}

function coral_get_messages() {
  $output = '';
  global $messages;
  if (!empty($messages)) {
    foreach ($messages as $message) {
      $output .= coral_template('message',
        array('message' => $message['content'],
          'type' => $message['type']), 'templates/system');
    }
  }
  return coral_template('messages', array('content' => $output),
    'templates/system');
}






