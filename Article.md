# Introduction #
Article is an object of the [world](World.md). [Creatures](Creature.md) can do their [activities](Activity.md) using articles. Every article have some properties:
  * when it expire (article older then this value disapear)
  * volume (amount of place taken by article in the [field stocks](Field#Stock.md) or [creature backpack](Creature#Backpack.md)
  * way in which article influences creatures/fields during actions
Articles are components of [recipes](Article#Recipe.md).

# Recipe #
Recipe defines the way of producing one article from others articles.
It contains
  * name of product which will be produced
  * amount of product which will be produced
  * list of names and materials needed to complete production
