# -*- coding: utf-8 -*-


require 'xot/setter'
require 'xot/universal_accessor'
require 'xot/block_util'
require 'reflex/ext'
require 'reflex/helper'


module Reflex


  class Shape

    include Xot::Setter
    include Hookable
    include HasFrame
    include HasTags

    alias sensor? sensor

    universal_accessor :name, :selector,
      :density, :friction, :restitution, :sensor,
      :category_bits, :collision_mask

    def initialize (options = nil, &block)
      super()
      set options if options
      Xot::BlockUtil.instance_eval_or_block_call self, &block if block
    end

  end# Shape


end# Reflex
