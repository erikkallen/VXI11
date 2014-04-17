#VXI11 Ruby Library by Erik Kallen

Based on the vxi11 code by Mike Marino (https://github.com/mgmarino/VXI11Discovery)

This library allows you to control VXI11 capable measurement devices like scopes, sine generators from Tektronix LeCroy R&S etc.
It can find devices on your network and talk to them directly.

###Installing
To install this gem you can add it to your gemfile or install using rubygems

#####Gemfile

gem 'vxi11'


#####Rubygems

gem install vxi11


###Example

    require 'vxi11'

    v = VXI11.new
    v.find_devices
    v.connect "10.0.0.1"
    puts v.send_and_receive "*IDN?"