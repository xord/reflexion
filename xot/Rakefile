# -*- mode: ruby; coding: utf-8 -*-


%w[.]
  .map  {|s| File.expand_path "../#{s}/lib", __FILE__}
  .each {|s| $:.unshift s if !$:.include?(s) && File.directory?(s)}

require 'xot/rake'
require 'xot/module'

include Xot::Rake


MODULES = [Xot].map {|m| m.const_get :Module}
MODULE  = MODULES.last
DLNAME  = 'tester'


task :default => :build

task :build => :lib

empty_task :test


MODULES.each {|m| m.load_tasks :lib, :ext, :test, :gem}
