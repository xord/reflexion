# -*- mode: ruby; coding: utf-8 -*-


%w[.]
  .map  {|s| File.expand_path "#{s}/lib", __dir__}
  .each {|s| $:.unshift s if !$:.include?(s) && File.directory?(s)}

require 'xot/rake'

require 'xot/module'


MODULES = [Xot]
DLNAME  = 'tester'

build_native_library
build_ruby_extension dlname: DLNAME
test_ruby_extension
build_ruby_gem

task :default => :test
