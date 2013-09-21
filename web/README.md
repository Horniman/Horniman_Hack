This folder contains the web front end code.


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

    sudo apt-get update
    sudo apt-get install nginx php5-fpm

Then edit the Nginx configuration file found at
`/etc/nginx/sites-enabled/default`
so it looks something like this:

    server {
      root /vagrant/build/Horniman_Hack/web/public_html;
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

You might need to change the root location depending on where you have git
cloned the code base.`
Then just start or restart the services:

   sudo service nginx stop
   sudo service php5-fpm restart
   sudo service nginx start

Done!










