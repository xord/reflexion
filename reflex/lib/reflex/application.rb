# -*- coding: utf-8 -*-


require 'xot/setter'
require 'xot/block_util'
require 'reflex/ext'
require 'reflex/helper'


module Reflex


  class Application

    include Xot::Setter
    include Hookable

    def initialize (opts = {}, &block)
      super()
      set opts
      @start_block = block if block
    end

    def self.start (*args, &block)
      self.new(*args, &block).start
    end

    private

      def call_start_block ()
        return unless @start_block
        Xot::BlockUtil.instance_eval_or_block_call self, &@start_block
        @start_block = nil
      end

  end# Application


end# Reflex
