# -*- mode: ruby; coding: utf-8 -*-


%w[../xot .]
  .map  {|s| File.expand_path "../#{s}/lib", __FILE__}
  .each {|s| $:.unshift s if !$:.include?(s) && File.directory?(s)}

require 'xot/rake'
require 'xot/module'
require 'rucy/module'

include Xot::Rake


MODULES    = [Xot, Rucy].map {|m| m.const_get :Module}
MODULE     = MODULES.last
DLNAME     = 'tester'
INCDIRS    = ruby_incdirs
RUCY2RDOC  = 'bin/rucy2rdoc'
NPARAM_MAX = 8
NTIMES     = (0..NPARAM_MAX)


task :default => :build

task :build => :lib


MODULES.each {|m| m.load_tasks :lib, :ext, :test, :doc, :gem}
