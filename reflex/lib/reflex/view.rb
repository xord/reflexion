# -*- coding: utf-8 -*-


require 'forwardable'
require 'xot/setter'
require 'xot/universal_accessor'
require 'xot/block_util'
require 'reflex/ext'
require 'reflex/model_view'
require 'reflex/helper'


module Reflex


  class View

    include Xot::Setter
    include Hookable
    include HasFrame
    include HasTags

    extend Forwardable

    alias add    add_child
    alias remove remove_child
    alias find   find_children
    alias meter  meter2pixel
    alias debug? debug

    def_delegators :style,
      :flow=,   :flow,
      :fill=,   :fill,
      :stroke=, :stroke,
      :image=,  :image

    def_delegators :body,
      :static=,           :static,  :static?,
      :dynamic=,          :dynamic, :dynamic?,
      :density=,          :density,
      :friction=,         :friction,
      :restitution=,      :restitution,
      :velocity=,         :velocity,
      :linear_velocity=,  :linear_velocity,
      :angular_velocity=, :angular_velocity,
      :gravity_scale=,    :gravity_scale,
      :apply_force,
      :apply_torque,
      :apply_impulse,
      :apply_linear_impulse,
      :apply_angular_impulse

    bit_flag_accessor :capture do
      flag :key,     CAPTURE_KEY
      flag :pointer, CAPTURE_POINTER
      flag :all,     CAPTURE_ALL
    end

    universal_accessor :name, :selector, :frame, :zoom, :capture, :gravity, :debug

    def initialize (options = nil, &block)
      super()
      set options if options
      @attach_block = block if block
    end

    def children ()
      to_enum :each_child
    end

    def styles ()
      to_enum :each_style
    end

    def style (*args, &block)
      s = get_style args.empty? ? nil : Selector.selector(*args)
      Xot::BlockUtil.instance_eval_or_block_call s, &block if block
      s
    end

    def capturing? (*args)
      cap = capture
      args.all? {|type| cap.include? type}
    end

    def self.has_model ()
      include ModelView
    end

    private

      def call_attach_block ()
        return unless @attach_block
        Xot::BlockUtil.instance_eval_or_block_call self, &@attach_block
        @attach_block = nil
      end

  end# View


end# Reflex
