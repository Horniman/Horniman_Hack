<?php

// Pull in shared code
require '../app/core.php';

// Bootstrap (i.e. connect DB etc)
coral_bootstrap();

// Process the request
coral_execute();
