# -*- mode: ruby; coding: utf-8 -*-


%w[../xot ../rucy ../rays .]
  .map  {|s| File.expand_path "../#{s}/lib", __FILE__}
  .each {|s| $:.unshift s if !$:.include?(s) && File.directory?(s)}

require 'xot/rake'
require 'xot/module'
require 'rucy/module'
require 'rays/module'
require 'reflex/module'

include Xot::Rake


MODULES     = [Xot, Rucy, Rays, Reflex].map {|m| m.const_get :Module}
MODULE      = MODULES.last
GEMNAME     = 'reflexion'
INCDIRS     = ['src/physics']
TESTS_ALONE = ['test/test_reflex.rb']


task :default => :build

task :build => :ext


MODULES.each {|m| m.load_tasks :lib, :ext, :test, :doc, :gem, :box2d}
