<?php

/**
 * @file
 * The main core functions used by all pages
 *
 */

/**
 * Bootstrap the system
 */
function leak_bootstrap() {
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

  // TODO: check controller and op are in the allowed values
  $request = leak_parse_request();
  leak_debug('request', $request);

}


/**
 * Execute the current request
 */
function leak_execute() {
  global $request, $user, $config;

  require_once "{$request['controller']}/{$request['controller']}.inc";
  $access_callback = "{$request['controller']}_access";
  if (function_exists($access_callback) && $access_callback($user)) {
    $callback = "{$request['controller']}_{$request['op']}_execute";
    if (function_exists($callback)) {
      $vars = $callback();

      // Setup some common page variables
      $vars['page_title'] = 'LEAK';
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
  return leak_template('debug', $vars, 'system');
}

/**
 * Convert some variables to rendered output
 */
function leak_template($template_name, $variables, $folder = '') {
  extract($variables, EXTR_SKIP);
  // Start output buffering
  ob_start();
  // If a subfolder name is provided add it to the template name
  if (!empty($folder)) {
    $template_name  = $folder . '/' . $template_name;
  }
  // Include the template file
  include 'templates/' . $template_name . '.tpl.php';
  // End buffering and return its contents
  return ob_get_clean();
}


function leak_parse_request() {
  global $config;
  $request = array();

  $path = $_SERVER['REQUEST_URI'];

  // Strip of format if requested
  $path_info = pathinfo($path);
  if (!empty($path_info['extension'])) {
    $request['format'] = $path_info['extension'];
    $path = substr($path, 0, strlen($path) - strlen($path_info['extension']) - 1);
  }
  else {
    $request['format'] = 'html';
  }

  // Strip any GET parameters
  if ($remove_get = stristr($path, '?', TRUE)) {
    $path = $remove_get;
  }

  // Split path
  $path = explode('/', $path);

  // Extract controller and operation
  if (empty($path[1])) {
    $request['controller'] = $config['default_controller'];
  }
  else {
    $request['controller'] = $path[1];
  }

  if (empty($path[2])) {
    $request['op'] = $config['default_op'];
  }
  else {
    $request['op'] = $path[2];
  }

  // Is debug mode requested?
  if (isset($_GET['debug'])) {
    $config['debug'] = TRUE;
  }
  leak_debug('rS', $_SERVER);
  leak_debug('rG', $_GET);
  return $request;
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
    $output['content'] = leak_template('login', array(), 'system');
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








