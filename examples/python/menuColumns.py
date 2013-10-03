# Create a sub-menu that will contain the multiple columns
mm = MenuManager.createAndInitialize()
menu2 = mm.getMainMenu().addSubMenu("TestMenu")
 
# Get the menu container, set its layout to horizontal. Set all elements 
# aligned at the top of the container
mc = menu2.getContainer()
mc.setLayout(ContainerLayout.LayoutHorizontal)
mc.setVerticalAlign(VAlign.AlignTop)
 
# Create 3 vertical columns, add them to the menu
c1 = Container.create(ContainerLayout.LayoutVertical, mc)
c2 = Container.create(ContainerLayout.LayoutVertical, mc)
c3 = Container.create(ContainerLayout.LayoutVertical, mc)
 
# Add a bunch of buttons to the columns
# Column 1
b1c1 = Button.create(c1)
b2c1 = Button.create(c1)

 
# Column 2
b1c2 = Button.create(c2)
b2c2 = Button.create(c2)
b3c2 = Button.create(c2)
 
# Column 3
b1c3 = Button.create(c3)
b2c3 = Button.create(c3)
b3c3 = Button.create(c3)

# setup navigation
# vertical navigation for buttons will be set up automatically by the container.
# we set horizontal navigation manually

# Buttons on the first row
b1c1.setHorizontalNextWidget(b1c2)
b1c2.setHorizontalPrevWidget(b1c1)

b1c2.setHorizontalNextWidget(b1c3)
b1c3.setHorizontalPrevWidget(b1c2)

# Buttons on the second row
b2c1.setHorizontalNextWidget(b2c2)
b2c2.setHorizontalPrevWidget(b2c1)

b2c2.setHorizontalNextWidget(b2c3)
b2c3.setHorizontalPrevWidget(b2c2)

# Buttons on the third row
# since the first column does not have a third row, prev navigation
# from column 2 goes to the last button of column 1
b3c2.setHorizontalPrevWidget(b2c1)

b3c2.setHorizontalNextWidget(b3c3)
b3c3.setHorizontalPrevWidget(b3c2)


