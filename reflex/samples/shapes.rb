# -*- coding: utf-8 -*-


%w[xot rays reflex]
  .map  {|s| File.expand_path "../../../#{s}/lib", __FILE__}
  .each {|s| $:.unshift s if !$:.include?(s) && File.directory?(s)}

require 'reflex'

include Reflex


win = Window.new do
  set title: 'Shape Sample', frame: [100, 100, 500, 400]

  p = painter
  p.background 0
  p.fill 1

  %w[rect ellipse arc].each.with_index do |shape, i|
    add Reflex.const_get("#{shape.capitalize}Shape").new {
      set frame: [32 + 64 * i, 32, 50, 50], fill: :gray, stroke: :white
    }
  end
end


Reflex.start do
  win.show
end
