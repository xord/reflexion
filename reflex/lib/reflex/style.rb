# -*- coding: utf-8 -*-


require 'xot/setter'
require 'xot/universal_accessor'
require 'xot/block_util'
require 'reflex/ext'
require 'reflex/helper'


module Reflex


  class Style

    include Xot::Setter
    include HasTags

    def initialize (options = nil, &block)
      super()
      set options if options
      Xot::BlockUtil.instance_eval_or_block_call self, &block if block
    end

    def flow= (*syms)
      set_flow *syms.flatten.map {|sym| sym2flow sym}
    end

    def flow ()
      get_flow.map {|n| flow2sym n}
    end

    universal_accessor :name, :selector, :flow, :width, :height, :size,
              :left,         :top,         :right,         :bottom, :position,
       :offset_left,  :offset_top,  :offset_right,  :offset_bottom, :offset,
       :margin_left,  :margin_top,  :margin_right,  :margin_bottom, :margin,
      :padding_left, :padding_top, :padding_right, :padding_bottom, :padding,
      :fill, :stroke, :image

    def inspect ()
      attrs = {
        selector: selector,
        flow:     flow,
        size:     size,
        position: position,
        offset:   offset,
        margin:   margin,
        padding:  padding,
        fill:     fill,
        stroke:   stroke,
        image:    image
      }
      "#<Reflex::Style #{attrs.map {|k, v| %(#{k}:#{v.to_s})}.join ' '}>"
    end

    private

      SYM2FLOW = {
        none:  FLOW_NONE,
        down:  FLOW_DOWN,
        right: FLOW_RIGHT,
        up:    FLOW_UP,
        left:  FLOW_LEFT,
      }

      FLOW2SYM = SYM2FLOW.inject({}) {|h, (k, v)| h[v] = k; h}

      def sym2flow (sym)
        SYM2FLOW[sym] || FLOW_NONE
      end

      def flow2sym (flow)
        FLOW2SYM[flow] || :none
      end

  end# Style


end# Reflex
