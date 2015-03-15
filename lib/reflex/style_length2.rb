# -*- coding: utf-8 -*-


require 'xot/setter'
require 'xot/block_util'
require 'reflex/ext'


module Reflex


  class StyleLength2

    include Xot::Setter

    alias w= width=
    alias w  width
    alias h= height=
    alias h  height

    def initialize (opts = {}, &block)
      super()
      set opts
      Xot::BlockUtil.instance_eval_or_block_call self, &block if block
    end

    def to_a ()
      [width, height]
    end

  end# StyleLength2


end# Reflex
