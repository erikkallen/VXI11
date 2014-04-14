#!/usr/bin/env ruby

require './vxi11'

t = VXI11.new

puts t.find_devices

t.connect "192.168.47.157"

puts t.send_and_receive("DATA:FFT:X?")