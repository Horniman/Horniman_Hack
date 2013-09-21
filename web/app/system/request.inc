<?php

/**
 * @file
 * This file contains function for parsing the request and
 * generating the global $request array.
 */

/**
 *
 * @return an array containing the controller, operation
 * request method and format
 */
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
  if (!in_array($request['controller'], $config['controllers'])) {
    die('Unknown controller');
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
  return $request;
}
