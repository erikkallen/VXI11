Gem::Specification.new do |s|
  s.name    = "vxi11"
  s.version = "0.1.0"
  s.summary = "VXI11 for Ruby"
  s.author  = "Erik Kallen"
  
  s.files = Dir.glob("ext/**/*.{c,rb,cc}") +
            Dir.glob("lib/**/*.rb")
  
  s.extensions << "ext/vxi11/extconf.rb"
  
  s.add_development_dependency "rake-compiler"
  s.add_dependency "rice"
end