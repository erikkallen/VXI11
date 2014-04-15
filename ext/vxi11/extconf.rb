require 'mkmf-rice'

ENV['ARCHFLAGS']= "-Wno-error=unused-command-line-argument-hard-error-in-future"

create_makefile('vxi11/vxi11')
