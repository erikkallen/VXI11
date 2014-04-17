Gem::Specification.new do |s|
  s.name    = "vxi11"
  s.version = "0.1.2"
  s.summary = "VXI11 protocol library for Ruby"
  s.author  = "Erik Kallen"
  s.email   = "info@erikkallen.nl"
  s.description = "This library allows you to control VXI11 capable measurement devices like scopes, sine generators from Tektronix LeCroy R&S etc.
It can find devices on your network and talk to them directly."
  s.homepage = "https://github.com/erikkallen/VXI11"
  s.licenses = ['GPL']
  
  s.files = Dir.glob("ext/**/*.{c,rb,cc}") +
            Dir.glob("lib/**/*.rb") + 
            Dir.glob("lib/.rb")
            
  s.require_paths = [ 'lib', 'ext' ]
  
  s.extensions << "ext/vxi11/extconf.rb"
  
  s.add_development_dependency "rake-compiler"
end