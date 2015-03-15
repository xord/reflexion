# -*- coding: utf-8 -*-


%w[.]
  .map  {|s| File.expand_path "../../#{s}/lib", __FILE__}
  .each {|s| $:.unshift s if !$:.include?(s) && File.directory?(s)}

require 'test/unit'
require 'xot'
require 'xot/test'
require 'xot/tester'

include Xot::Test
