require 'rubygems'

Gem::Specification.new do |spec|
  spec.name = 'tclink'
  spec.version = '4.5.0'
  spec.summary = "TCLink Trust Commerce link"
  spec.description = "Trust Commerce connectivity layer"
  spec.homepage = "https://github.com/peejaybee/TCLink"
  spec.license = "LGPL-2.1"
  spec.require_path = 'ext/tclink'
  spec.required_ruby_version = '>=2.6.5'
  spec.authors = ["Josh Puetz", "Phillip Birmingham", "Patrick Logan"]
  spec.extensions = ["ext/tclink/extconf.rb"]
  spec.files = Dir['ext/tclink/*.c'] + Dir['ext/tclink/*.h'] + Dir['ext/tclink/*.rb'] + %w[
    ext/tclink/LICENSE 
    ext/tclink/README
    ext/tclink/depend
    ext/tclink/build.sh
    tclink.gemspec]
end
