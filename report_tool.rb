#!/usr/bin/env ruby

require './ext/vxi11/vxi11'

t = VXI11.new "192.168.47.157"


puts t.find_devices.inspect


#puts t.send_and_receive("DATA:FFT:X?")

puts t.send_and_receive("*IDN?")


t.send("*IDN?")

puts t.receive