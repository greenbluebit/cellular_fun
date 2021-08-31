<img align="left" src="https://media2.giphy.com/media/YFoZ1yAgqegu6pekdc/giphy.gif?cid=790b7611fe1cf74a6a5893f5e8c4967368b77a3b7a09e1d0&rid=giphy.gif&ct=g" width="288px">

**Cellular Fun is a small cellular automata 'engine'**

It allows users to design and test their own cellular automatas with no coding. It is well suited for trying out new ideas and experiencing the interesting patterns developing over generations.

It supports both self actualizing cellular automata designs, neighbour affecting ones (Think Sand or Water) and combining the two using the relationships for each cell type.

Designed cells can be saved for later use or sent to other users in .json format.

The code is available in this repo, if anyone wants to change anything please go ahead.

The following text will explain how each feature works.

---

<br>

<a href="https://www.linkedin.com/in/theodorpurcaru/">LinkedIn</a> | 
<a href="https://greenbluebit.github.io/">Portfolio</a> | 
<a href="https://twitter.com/ted_f_p">Twitter?</a>


How it works
--------

<img align="center" src="https://i.ibb.co/cr8bb9X/UI.png" alt="Brian's Brain running" title="Brian's Brain running">
<p align="center">Brian's Brain running with the 3 required cell types and their relationships.</p>

The application works similarly to a tile based game, users interact with the grid by selecting cells and "drawing" them. Both the cells as well as the controls affecting the grid are on the right side.

<br>

<img align="right" src="https://i.ibb.co/gyf6N5R/left-ui.png" width="90px">

The game begins with one cell which covers all of the grid, this is the Default Cell.

Existing cells can be edited by pressing the "..." button.

New cells can be added by pressing the "+" button.

There can be a maximum of 10 cells.

<br><br>

<img align="right" src="https://i.ibb.co/zZVwD1x/left-ui-bottom.png" width="150px">

The **Wrap checkbox** changes whether the cells touching the limits of the grid affect or are affected by neighbouring cells at the other end. This can be useful to simulate a nearly infinite board as cells will always have neighbours and not hit the border.

The **Generate button** uses all of the setup cells and randomly spreads them across the grid.

The **Medium button** is the current size of the brush. By default it starts as "Small". The current possible values are "Small", "Medium", "Large", each of them increasing the affected cells on the grid when "drawing" on it by 1.

The **Step button** ages the grid cells by one generation. Running the logic using all of the relationships.

The **Slow button** is the current speed of the simulation, affecting how quickly generations pass. By default it starts as "Stop". The current possible values are "Stop", "Slowest", "Slow", "Fast", "Fastest".

The **Clear button** wipes the grid, setting all of the cells to the Default cell.

The **Pause button** stops the simulation and opens the main menu.

<br><br>

<img align="left" src="https://i.ibb.co/NxfmWDQ/left-ui-menu.png" width="200px">

The main menu appears on the left and is separated in two sections, Rules and Images.

Rules involve the designed cell types, they can be saved or loaded in .json format.

The grid state can be saved as an image and later loaded back.

Loading works by taking each pixel, and finding the closest cell type by color, and using it in the location on the grid corresponding to the pixel location. It uses the **Pythagoras Theorem**. Currently any image larger than 360x190 has the rest of the pixels ignored.

Pressing any of the four buttons to load or save something will open the File Dialog.

<br><br><br><br><br><br><br><br><br><br><br><br><br>

<p align="center"><img src="https://i.ibb.co/W6zCpsr/file-dialog.png" width="700px" align="center"></p>
<p align="center">The File Dialog showing all of the existing rules in .json format</p>

The dialog allows a user to decide what to save and where. Currently, the user needs to specify the format, ".json" for rules and ".png", ".jpeg", ".jpg", "bmp" for images.

How to Add/Edit a Cell
--------

When pressing the **...** button or **+** button, the Cell Editor Dialog will appear. It is separated in two principal parts.

<p align="center"><img src="https://i.ibb.co/q1HGKyP/create-cell-top.png" width="520px" align="center"></p>

The top part partains to the global properties of a cell. From all of the options here, only the name is required, everything else is optional.

The **Is Default checkbox** sets the current cell to be default. This is used when clearing the grid to swap all other cells on the grid with this one.

The **Is Immovable checkbox** sets the current cell to be immovable. This makes it's relationships redundant and also blocks other cells from switching positions with it. An example of this would be Acid being able to move through any other cell, but not being able to move the cell above it like water may be able to do, instead "destroying" it.

The **Is Maintaining Velocity checkbox** makes the cell looking for free directions to move in to always prioritize the current direction if one exists. This makes for example liquids move faster by not alternating left and right.

The **Color picker** on the right is what we use for the Image loading/saving functionality as well as how we differentiate between cells in the menu on the right of the window.

The **Add Relationship button** adds a new relationship to the current cell. It can only be done 10 times.

<p align="center"><img src="https://i.ibb.co/MMhsmHC/create-cell-bottom.png" width="520px" align="center"></p>

The table in the bottom of the Cell Editor Dialog consists of the relationships active in the current cell.

The **Type column** shows whether the relationship focuses on changing the cell itself based on what is around it, or changing its neighbours. If it is setup to change it's neighbours, then the first neighbour that fulfills the relationship will become the current cell.

The **Target column** shows on what cell neighbour will the relationship work. It can have the value "Any" which means it would work on any cell. This is useful when you have generic relationships and don't want to make one for every planned cell. This is also why the **Is Immovable checkbox** exists, since the user may want to move through all of the cells, but not turn some of them into "Empty" or anything else.

The **Result column** shows what the current cell will result if the relationship will be satisfied. Regardless if the relationship is set to affect the cell itself or the neighbours, the result will be applied on the current cell.

The **Compare column** shows what operation should be run in order for the relationship to be satisfied. This is not usable when the **relationship type** is set to be **Neighbour**. The possible values are "=", "!=", "<", "<=", ">", ">=", ".", ">=<=". The "." implies self, meaning the relationship will not look for any neighbours but will simply change the current cell itself to the cell in **Result column**. This can be useful for example in case you have a number of survival relationships, all fail, to force the current cell to die.

The **Amount column** is used when the **relationship type** is set to "Self" in order to know how many neighbours are needed to compare against for the relationship to be fulfilled. If the **compare** is set to ">=<=", then two numbers will be used.

The **Neighbours column** allows users to specify exactly where to look for neighbours. This allows for very intricate behaviours where one relationship may work only diagonally while another in a Moore Neighbourhood style.

The **Change column** allows the user to decide the odds that the current relationship will attempt to operate. If set to "100" it will always work.

The **Delete column** allows the user to remove a relationship.

The **Save button** creates the cell, selects it as the current cell used by the brush and unpauses the simulation, hiding the **main menu**. If the **Generation Speed** was already set to "Stop" then the simulation will not move.

The **Delete button** allows the user to remove a cell. This does not work if the cell is currently used in a relationship by another cell and will require the user to first remove the cell from said relationship.

Comfort Tricks
--------

Most of the functionality described can be accessed via keyboard keys directly. The following image shows all of the binded keys:

<p align="center"><img src="https://i.ibb.co/WPDTMr7/keyboard-usage-diagram.png" width = "1600px" align="center"></p>

**Cinematic Mode** hides the UI.

What can be done with it
--------

<img src="https://media3.giphy.com/media/1t49rzDw6R2n4fULtE/giphy.gif?cid=790b7611cd512d26d4b7c741e8548b222ab66907fb9a679a&rid=giphy.gif&ct=g" width="1440px">
<p align="center">Brian's Brain</p>
<br>
<img src="https://media0.giphy.com/media/uwq8I7VloDgScBx4mC/giphy.gif?cid=790b761117074dc5d1b863939c1ec0a7a3134221aa0b47bd&rid=giphy.gif&ct=g" width="1440px">
<p align="center">Mazelectric</p>
<br>
<img src="https://media1.giphy.com/media/VAbEqx3WmlMeTzArjI/giphy.gif?cid=790b761164cdc884fcf6a6343be0e502b8896b0bfa776c35&rid=giphy.gif&ct=g" width="1440px">
<p align="center">WireWorld</p>
