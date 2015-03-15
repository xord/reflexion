# -*- coding: utf-8 -*-


%w[../xot .]
  .map  {|s| File.expand_path "../../../#{s}/lib", __FILE__}
  .each {|s| $:.unshift s if !$:.include?(s) && File.directory?(s)}

require 'mkmf'
require 'xot/extconf'
require 'xot/module'
require 'rucy/module'


Xot::ExtConf.new Xot, Rucy do
  setup do
    headers    << 'boost/noncopyable.hpp' << 'ruby.h'
    local_libs << 'rucy'
  end

  dir_config 'boost'
  create_makefile 'rucy/tester'
end
