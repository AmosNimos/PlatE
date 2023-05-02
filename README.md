<h1>PlatE Game Engine</h1>
<img src="Level_screenshot.png">
<p>PlatE is a game engine for creating platformer games. It is currently under the MIT license.</p>
	<h2>Creating Levels</h2>
	<p>To create a level for PlatE, create a text file with the level design. The level should be a grid of characters, where each character represents a different object in the game. The following objects are currently supported:</p>
	<ul>
		<li>"@": Player</li>
		<li>"#": Solid block</li>
		<li>"_": Platform (with no bottom collision)</li>
		<li>".": Empty</li>
		<li>"C": Coin</li>
		<li>"G": Goal</li>
		<li>"X": Spike</li>
	</ul>
	<p>The text file should have a fixed width and height. Each character should be separated by a space or a newline.</p>
	<h2>Compiling and Using PlatE</h2>
	<p>To compile PlatE, run the following command:</p>
	<pre>gcc PlatE.c -o PlatE -lSDL2</pre>
	<p>To use PlatE, run the following command:</p>
	<pre>./PlatE level_name.lvl</pre>
	<p>In this example <code>level_name.lvl</code> file would be the name of the text file containing the level design.</p>
	<h2>How it Works</h2>
	<p>PlatE is written in C and uses the SDL2 graphics library for rendering graphics. The current version of PlatE includes the following features:</p>
	<ul>
		 <li>Level creation from a text file</li>
	    <li>Basic collision detection and gravity</li>
	    <li>Player movement and jumping</li>
	    <li>Objects with customizable color and solidness</li>
	    <li>A maximum of 300 objects allowed in the game</li>
	    <li>A game over and win game function</li>
	</ul>
	<p>PlatE is still under development, and additional features are planned for future versions:</p>
	<ul>
		<li>Viewport</li>
		<li>Sound</li>
		<li>Adaptive level size based on the .lvl file</li>
		<li>Multiple levels</li>
		<li>Main menu</li>
		<li>Level editor</li>
		<li>Moving enemies</li>
		<li>Moving platforms</li>
		<li>Power-ups</li>
	</ul>

