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

      x, y, y2, w, h = 10, 10, 100, 50, 50

      push do
        rect x, y, w, h
        line Rays::Polygon.rect x, y2, w, h

        translate 100, 0
        rect x, y, w, h, 5
        line Rays::Polygon.rect x, y2, w, h, 5

        translate 100, 0
        rect x, y, w, h, 5, 10, 15, 20
        line Rays::Polygon.rect x, y2, w, h, 5, 10, 15, 20
      end

      translate 0, 200
      push do
        ellipse x, y, w, h
        line Rays::Polygon.ellipse x, y2, w, h

        translate 100, 0
        ellipse x, y, w, h, from: 45, to: 180
        line Rays::Polygon.ellipse x, y2, w, h, from: 45, to: 180

        translate 100, 0
        ellipse x, y, w, h, hole: 10
        line Rays::Polygon.ellipse x, y2, w, h, hole: 10

        translate 100, 0
        ellipse x, y, w, h, hole: 10, from: 200, to: 300
        line Rays::Polygon.ellipse x, y2, w, h, hole: 10, from: 200, to: 300
      end

      translate 0, 200
      push do
        line x, y, x + w, y + h

        x += 100
        line x, y, x + w, y + h, x, y + h

        x += 100
        line x, y, x + w, y + h, x, y + h, loop: true
      end
    end
  end
end.show


Reflex.start
