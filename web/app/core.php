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
  // TODO: Parse the request
  // TODO: Connect to the database
  // TODO: Load the current user
}


/**
 * Execute the current request
 */
function leak_execute() {
  // TODO: Check permissions by defined access callback
  // TODO: Execute the request via defined callback
  // TODO: Format the response

  // Temporary, just to check everything is working, this will be removed:
  require_once 'test/test.inc';
  $vars = test_request();
  $vars['page_title'] = 'LEAK';

  // Format output depending on requested format
  // (hardcode to html initially)
  $format = 'html';
  switch ($format) {
    case 'html':
      echo leak_template('html', $vars);
      break;
  }
}


/**
 * Convert some variables to rendered output
 */
function leak_template($template_name, $variables) {
  extract($variables, EXTR_SKIP);
  // Start output buffering
  ob_start();
  // Include the template file
  include 'templates/' . $template_name . '.tpl.php';
  // End buffering and return its contents
  return ob_get_clean();
}






