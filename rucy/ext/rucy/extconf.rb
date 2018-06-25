# -*- coding: utf-8 -*-


%w[../xot .]
  .map  {|s| File.expand_path "../../#{s}/lib", __dir__}
  .each {|s| $:.unshift s if !$:.include?(s) && File.directory?(s)}

require 'mkmf'
require 'xot/extconf'
require 'xot/module'
require 'rucy/module'


Xot::ExtConf.new Xot, Rucy do
  setup do
    headers    << 'ruby.h'
    local_libs << 'rucy'
  end

  create_makefile 'rucy/tester'
end
