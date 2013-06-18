# -*- mode: ruby -*-
# vi: set ft=ruby :

pgbox    = ENV['BOX'] || "precise64"

Vagrant.configure("2") do |config|

  # Vagrant box configuration
  config.vm.box = pgbox
  config.vm.box_url = "http://files.vagrantup.com/%s.box" % [pgbox]

  # Bootstrap script
  config.vm.provision :shell, :path => "tools/vagrant/bootstrap.sh"

  # Forward SSH agent to host
  config.ssh.forward_agent = true

  # Create synced folder for GnuPG keys
  config.vm.synced_folder "~/.gnupg", "/home/vagrant/.gnupg"
  config.vm.synced_folder "../", "/home/vagrant/repos"

end
