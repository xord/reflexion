# -*- coding: utf-8 -*-


%w[../xot ../rucy ../rays .]
  .map  {|s| File.expand_path "../../#{s}/lib", __FILE__}
  .each {|s| $:.unshift s if !$:.include?(s) && File.directory?(s)}

require 'test/unit'
require 'xot/test'
require 'reflex'

include Xot::Test


unless $REFLEX_NOAUTOINIT
  def Rays.fin!() end
  def Reflex.fin!() end
end
