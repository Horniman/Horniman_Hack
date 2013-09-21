This folder contains the web front end code.

## Methodology

The public_html folder includes the front end, web accessible code and is
where nginx points to. There is an index.php file that nginx redirects all
requests to (that don't match an actual static file).

    coral_bootstrap()

This sets up the DB connection, loads the current user, and parses the
request.

    coral_execute()

This passes control to the appropriate callback function. Callbacks must
be enabled in the configuration file config.ini and then exist within the
controllers folder.

The result of the controller function either passed to the template function
to create the full HTML page, or encoded as JSON, depending on the selected
format in the request.

For example:

    http://192.168.33.10/dashboard/show

This shows the dashboard in HTML, but the data can be accessed as JSON using:

   http://192.168.33.10/dashboard/show.json


## Local dev instructions

We've got a simple dev machine setup using Vagrant. First create a Vagrant
configuration:

    vagrant box add precise32 http://files.vagrantup.com/precise32.box
    vagrant init precise32

Then edit the created configuration file to add the IP address. Your Vagrant
should look something like this:

    # -*- mode: ruby -*-
    # vi: set ft=ruby :
    Vagrant.configure("2") do |config|
      config.vm.box = "precise32"
      config.vm.network :private_network, ip: "192.168.33.10"
    end

Launch the machine and SSH in:

    vagrant up
    vagrant ssh

Then install nginx and PHP-FPM:

    vagrant@precise32: sudo apt-get update
    vagrant@precise32: sudo apt-get install nginx php5-fpm

Then edit the Nginx configuration file found at
/etc/nginx/sites-enabled/default
so it looks something like this:

    server {
      root /vagrant/Horniman_Hack/web/public_html;
      index index.php;
      server_name localhost;
      location / {
        try_files $uri $uri/ /index.html;
      }
      location ~ \.php$ {
        fastcgi_split_path_info ^(.+\.php)(/.+)$;
        fastcgi_pass 127.0.0.1:9000;
        fastcgi_index index.php;
        include fastcgi_params;
      }
    }

The folder in which you launched the virtual machine is mounted within the
vm at /vagrant so that you can point Nginx at the web codebase within the repo.

You might need to change the root location depending on where you have git
cloned the code base.`
Then just start or restart the services:

    vagrant@precise32: sudo service nginx stop
    vagrant@precise32: sudo service php5-fpm restart
    vagrant@precise32: sudo service nginx start

Now install MySQL on the vagrant box, create the database and import the test data.

When you've finished developing log out of SSH then either halt or destroy the machine:

    vagrant halt

or

    vagrant destroy







