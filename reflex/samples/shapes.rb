# -*- coding: utf-8 -*-


%w[xot rays reflex]
  .map  {|s| File.expand_path "../../../#{s}/lib", __FILE__}
  .each {|s| $:.unshift s if !$:.include?(s) && File.directory?(s)}

require 'reflex'


Reflex::Window.new do
  title 'Shape Sample'
  frame 100, 100, 500, 400

  def on_draw (e)
    e.painter.push do
      fill :pink
      stroke 1

      x, y, w, h = 10, 10, 50, 50
      rect x, y, w, h

      x += 100
      rect x, y, w, h, 5

      x += 100
      rect x, y, w, h, 5, 10, 15, 20

      x = 10
      y += 100
      ellipse x, y, w, h

      x += 100
      ellipse x, y, w, h, 45, 180

      x += 100
      ellipse x, y, w, h, 0, 360, 10

      x += 100
      ellipse x, y, w, h, 200, 300, 20

      x = 10
      y += 100
      line x, y, x + w, y + h

      x += 100
      line x, y, x + w, y + h, x, y + h
    end
  end
end.show


Reflex.start
