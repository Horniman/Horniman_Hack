<?php

print_r($_REQUEST);

// Pull in shared code
require '../app/core.php';

// Bootstrap (i.e. connect DB etc)
leak_bootstrap();

// Process the request
leak_execute();
