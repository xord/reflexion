# -*- coding: utf-8 -*-


require 'xot/setter'
require 'xot/block_util'
require 'reflex/ext'
require 'reflex/helper'


module Reflex


  class Selector

    include Xot::Setter
    include HasTags

    def initialize (opts = {}, &block)
      super()
      set opts
      Xot::BlockUtil.instance_eval_or_block_call self, &block if block
    end

    def inspect ()
      "#<#{self.class} name:'#{name}' tags:[#{tags.map{|s| "'#{s}'"}.to_a.join ','}]>"
    end

  end# Selector


end# Reflex
