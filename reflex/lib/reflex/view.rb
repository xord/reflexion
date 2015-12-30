# -*- coding: utf-8 -*-


require 'forwardable'
require 'xot/setter'
require 'xot/bit_flag_accessor'
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
      :sensor=,           :sensor,
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

    def initialize (options = nil, &block)
      super()
      set options if options
      @attach_block = block if block
    end

    def timeout (seconds, count: 1, &block)
      timer = start_timer seconds, count
      timer.block = block if block
      timer
    end

    def interval (seconds, &block)
      timeout seconds, count: -1, &block
    end

    def find_child (*args)
      find_children(*args).first
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

    def categories ()
      @categories ||= Xot::BitFlag.new(auto: true, all: 1)
    end

    def category= (*symbols)
      body.category = parent_categories.symbols2bits *symbols
    end

    def category ()
      parent_categories.bits2symbols body.category
    end

    def collision= (*categories)
      body.collision = parent_categories.symbols2bits *categories
    end

    def collision ()
      parent_categories.bits2symbols body.collision
    end

    def capturing? (*args)
      cap = capture
      args.all? {|type| cap.include? type}
    end

    alias add    add_child
    alias remove remove_child
    alias find   find_child
    alias meter  meter2pixel
    alias debug? debug

    universal_accessor :name, :selector, :frame, :zoom, :capture,
      :gravity, :category, :collision, :debug

    def self.has_model ()
      include ModelView
    end

    protected

      def parent_categories ()
        raise InvalidStateError unless parent
        parent.categories
      end

    private

      def call_attach_block ()
        return unless @attach_block
        Xot::BlockUtil.instance_eval_or_block_call self, &@attach_block
        @attach_block = nil
      end

  end# View


end# Reflex
