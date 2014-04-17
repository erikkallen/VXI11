#!/usr/bin/env ruby

require './ext/vxi11/vxi11'

t = VXI11::VXI11.new

#puts t.find_devices
t.connect "192.168.47.157"

#puts t.send_and_receive("DATA:FFT:X?")

puts t.send_and_receive("*IDN?")


t.send("*IDN?")

puts t.receive