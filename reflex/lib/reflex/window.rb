# -*- coding: utf-8 -*-


require 'forwardable'
require 'xot/setter'
require 'xot/universal_accessor'
require 'xot/block_util'
require 'reflex/ext'
require 'reflex/helper'


module Reflex


  class Window

    include Xot::Setter
    include Hookable
    include HasFrame

    extend Forwardable

    def_delegators :root,
      :add_child, :remove_child, :find_children,
      :style, :styles, :meter2pixel, :meter, :wall,
      :gravity=, :gravity,
      :debug=,   :debug, :debug?

    def_delegators :wall,
      :friction=,    :friction,
      :restitution=, :restitution

    def_delegators :style,
      :flow=,   :flow,
      :fill=,   :fill,
      :stroke=, :stroke,
      :image=,  :image

    universal_accessor :title, :frame

    alias add    add_child
    alias remove remove_child
    alias find   find_children

    def initialize (options = nil, &block)
      super()
      set options if options
      @show_block = block if block
    end

    def paint (&block)
      painter.begin &block
    end

    def self.show (*args, &block)
      new(*args, &block).show
    end

    private

      def call_show_block ()
        return unless @show_block
        Xot::BlockUtil.instance_eval_or_block_call self, &@show_block
        @show_block = nil
      end

  end# Window


end# Reflex
