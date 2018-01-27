# -*- coding: utf-8 -*-


require 'xot/setter'
require 'xot/universal_accessor'
require 'xot/block_util'
require 'reflex/ext'
require 'reflex/helper'


module Reflex


  class Application

    include Xot::Setter
    include Hookable

    universal_accessor :name

    def initialize (options = nil, &block)
      super()
      set options if options
      Xot::BlockUtil.instance_eval_or_block_call self, &block if block
    end

    def self.start (*args, &block)
      new(*args, &block).start
    end

  end# Application


end# Reflex
