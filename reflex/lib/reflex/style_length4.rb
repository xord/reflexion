# -*- coding: utf-8 -*-


require 'xot/setter'
require 'xot/block_util'
require 'reflex/ext'


module Reflex


  class StyleLength4

    include Xot::Setter

    alias l= left=
    alias l  left
    alias t= top=
    alias t  top
    alias r= right=
    alias r  right
    alias b= bottom=
    alias b  bottom

    def initialize (opts = {}, &block)
      super()
      set opts
      Xot::BlockUtil.instance_eval_or_block_call self, &block if block
    end

    def to_a ()
      [left, top, right, bottom]
    end

  end# StyleLength4


end# Reflex
