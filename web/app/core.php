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

  // Load the configuration
  $config = parse_ini_file('../config.ini');
  leak_debug('config', $config);

  // TODO: Load the current user
  $user = leak_active_user();

  // TODO: Parse the request, here just hard code to test:
  // TODO: check controller and op are in the allowed values
  $request = array(
    'controller' => 'test',
    'op' => 'show',
    'format' => 'html',
  );

  // TODO: Connect to the database
  // $db =
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
      $vars = leak_error();
    }
  }
  else {
    $vars = leak_access_denied();
  }

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
      echo json_encode($vars);
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

function leak_active_user() {
  $user = array(
    'uid' => 0,
  );
  return $user;
}


function leak_access_denied() {
  return array(
    'page_title' => 'Access Denied',
    'content' => 'Access Denied',
  );
}


