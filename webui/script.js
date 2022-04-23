var m_board = document.querySelector("chess-board");
var m_lastPosition;

var color = 1; // white first
var chess_text = document.getElementById("chess_text");
var socket = io();

const highlightStyles = document.createElement("style");
document.head.append(highlightStyles);
const whiteSquareGrey = "#a9a9a9";
const blackSquareGrey = "#696969";

function showPosition() {
	console.log("Current position as an Object:");
	console.log(m_board.position);

	console.log("Current position as a FEN string:");
	console.log(m_board.fen());
}

function loadboard() {
	m_board = document.querySelector("chess-board");
	setTimeout(function () {
		m_board.start();
	}, 10);
}

m_board.addEventListener("drop", (e) => {
	const { source, target, piece, newPosition, oldPosition, orientation } =
		e.detail;
	m_lastPosition = oldPosition;
	removeGreySquares();

	// Addressing castle
	if (
		piece[1] == "K" &&
		source[1] == target[1] &&
		Math.abs(source.charCodeAt(0) - target.charCodeAt(0)) == 2
	) {
		row = piece[0] == "w" ? 1 : 8;
		if (piece[0] == "w") {
			if (source.charCodeAt(0) > target.charCodeAt(0)) {
				m_board.move("a" + row + "-d" + row);
			} else {
				m_board.move("h" + row + "-f" + row);
			}
		} else {
			if (source.charCodeAt(0) > target.charCodeAt(0)) {
				m_board.move("a8-d8");
			} else {
				m_board.move("h8-f8");
			}
		}
	}

	// Addressing en passant
	if (
		piece[1] == "P" &&
		source[0] != target[0] &&
		m_board.position[target] == undefined
	) {
		var enPassantTile = target;
		if (piece[0] == "w") {
			enPassantTile = target[0] + (parseInt(target[1]) - 1);
		} else {
			enPassantTile = target[0] + (parseInt(target[1]) + 1);
		}
		m_board.position[enPassantTile] = undefined;
	}

	console.log("dropping " + piece + " : " + source + "-" + target);
	socket.emit("drop", piece, source, target);
});

m_board.addEventListener("drag-start", (e) => {
	const { source, piece, position, orientation } = e.detail;
	console.log("drag-start " + piece + " : " + source);
	socket.emit("dragStart", source);
});

function isNullOrWhitespaceOrTabs(text) {
	if (typeof text === "undefined" || text == null) return true;
	return text.replace(/\s/g, "").length < 1;
}

// Parses the current move "move" to compute which piece moves where.
// return movements as array  e.g. ["e4-e5"]
function parseMove(move) {
	const regex = /\w\d-\w\d/;
	const found = move.match(regex);

	if (found) {
		return found;
	}

	if (isNullOrWhitespaceOrTabs(move)) {
		return "null move";
	}
	move = move.toLowerCase();
	console.log(move);

	var takes;
	if (move[0] == "x") {
		takes = 1;
		// console.log("Takes");
		move = move.slice(1); // remove x
	}
	if (move[move.length - 1] == "+") {
		// console.log("Check");
		move = move.slice(0, move.length - 1); // remove +
	}

	// King
	if (move[0] == "r") {
		// console.log("r");
		move = move.slice(1); // remove r
		return moveKing(color, move, takes);
	}

	// Queen
	else if (move[0] == "d" && isNaN(move[1])) {
		// console.log("d");
		move = move.slice(1); // remove d
		return moveQueen(color, move, takes);
	}

	// Rook
	else if (move[0] == "t") {
		// console.log("t");
		move = move.slice(1); // remove t
		return moveRook(color, move, takes);
	}

	// Bishop
	else if (move[0] == "f" && isNaN(move[1])) {
		// console.log("f");
		move = move.slice(1); // remove f
		return moveBishop(color, move, takes);
	}

	// Knight or pawn
	else if (move[0] == "c" && isNaN(move[1])) {
		// console.log("c");
		move = move.slice(1); // remove c
		return moveKnight(color, move, takes);
	} else if (move[0] == "0") {
		if (move == "0-0") {
			// console.log("kingside castling");
			var line = color == 1 ? 1 : 8;
			return ["e" + line + "-" + "g" + line, "h" + line + "-" + "f" + line]; // king & rook
		} else if (move == "0-0-0") {
			// console.log("queenside castling");
			var line = color == 1 ? 1 : 8;
			return ["e" + line + "-" + "c" + line, "a" + line + "-" + "d" + line]; // king & rook
		}
	} else {
		// console.log("p");
		return movePawn(color, move, takes);
	}
}

socket.on("move", (move) => {
	move_parsed = parseMove(move);
	console.log("moving: ", move_parsed);
	m_board.move(move_parsed);
	color *= -1;
});

socket.on("unmakeLastMove", () => {
	console.log("unmakeLastMove");
	m_board.position = m_lastPosition;
	color *= -1;
});

function removeGreySquares() {
	highlightStyles.textContent = "";
}

function greySquare(square) {
	const highlightColor =
		square.charCodeAt(0) % 2 ^ square.charCodeAt(1) % 2
			? whiteSquareGrey
			: blackSquareGrey;

	highlightStyles.textContent += `
			chess-board::part(${square}) {
					background-color: ${highlightColor};
			}
	`;
}

function squareToCoordinates(square) {
	var letter = String.fromCharCode((square % 8) + 97);
	var number = Math.floor(square / 8) + 1;
	return letter + number;
}

socket.on("moveset", (moveset) => {
	console.log("moveset: ", moveset);
	moves = moveset.split(" ");
	for (var move of moves) {
		if (!parseInt(move)) continue;
		move = squareToCoordinates(move);
		console.log("move: ", move);
		greySquare(move);
	}
});

// socket.onAny((eventName, ...args) => {
//  console.log("all: ", eventName)
// });

function parseMoveDelayed(move, delay) {
	var promise = new Promise(function (resolve, reject) {
		window.setTimeout(function () {
			resolve(parseMove(move));
		}, delay);
	});
	return promise;
}

function moveKing(color, move, takes) {
	var piece_name = (color == 1 ? "w" : "b") + "K";
	var piece_pos = findPiece(piece_name)[0];
	var combination = piece_pos + "-" + move;
	// console.log(combination);
	return combination;
}

function moveQueen(color, move, takes) {
	var piece_name = (color == 1 ? "w" : "b") + "Q";
	var piece_pos = findPiece(piece_name)[0];
	var combination = piece_pos + "-" + move;
	// console.log(combination);
	return combination;
}

function moveRook(color, move, takes) {
	var piece_name = (color == 1 ? "w" : "b") + "R";
	var piece_pos = findPiece(piece_name);
	var combination;
	for (var i in piece_pos) {
		if (canRookMove(piece_pos[i], move)) {
			if (move.length == 3) move = move.slice(1);
			combination = piece_pos[i] + "-" + move;
		}
	}
	// console.log(combination);
	return combination;
}

function canRookMove(piece_pos, move) {
	var letter_piece = piece_pos.charAt(0);
	var number_piece = parseInt(piece_pos[1]);

	var letter_move = move.charAt(0);
	var number_move = parseInt(move[1]);

	var ambiguity = null; // if ambiguity, equals letter
	if (move.length == 3) {
		letter_move = move.charAt(1);
		number_move = parseInt(move[2]);
		ambiguity = move.charAt(0);
	}

	// adjacency ?
	if (number_piece != number_move && letter_piece != letter_move) return 0;

	// trace line
	for (
		var i = previousChar(letter_piece);
		i > letter_move;
		i = previousChar(i)
	) {
		if (piece(i + number_piece)) return 0; // line left
	}

	for (
		var i = nextChar(letter_piece);
		number_piece == number_move && i < letter_move;
		i = nextChar(i)
	) {
		if (piece(i + number_piece)) return 0; // line right
	}

	// trace column
	for (
		var i = parseInt(number_piece) - 1;
		letter_piece == letter_move && i > number_move;
		--i
	) {
		if (piece(letter_piece + i)) return 0; // column down
	}

	for (
		var i = parseInt(number_piece) + 1;
		letter_piece == letter_move && i < number_move;
		++i
	) {
		if (piece(letter_piece + i)) return 0; // column up
	}

	return ambiguity == null || letter_piece == ambiguity;
}

function moveBishop(color, move, takes) {
	// first Bishop found do the move, no ambiguity tolerated
	var piece_name = (color == 1 ? "w" : "b") + "B";
	var piece_pos = findPiece(piece_name);
	var combination;
	for (var i in piece_pos) {
		if (canBishopMove(piece_pos[i], move)) {
			combination = piece_pos[i] + "-" + move;
		}
	}
	// console.log(combination);
	return combination;
}

function canBishopMove(piece_pos, move) {
	// check diagonality
	if (!diagonality(piece_pos, move)) return 0;

	// upper diagonals
	var i, j;
	for (
		i = previousChar(piece_pos.charAt(0)), j = parseInt(piece_pos[1]) + 1;
		i > move.charAt(0) && j < move[1];
		i = previousChar(i), ++j
	) {
		// letter + number
		// console.log(i,j)
		if (piece(i + j)) return 0; // upper diagonal left
	}

	for (
		i = nextChar(piece_pos.charAt(0)), j = parseInt(piece_pos[1]) + 1;
		i < move.charAt(0) && j < move[1];
		i = nextChar(i), ++j
	) {
		// letter + number
		// console.log(i,j)
		if (piece(i + j)) return 0; // upper diagonal right
	}

	// lower diagonals
	for (
		i = previousChar(piece_pos.charAt(0)), j = parseInt(piece_pos[1]) - 1;
		i > move.charAt(0) && j > move[1];
		i = previousChar(i), --j
	) {
		// letter + number
		// console.log(i,j)
		if (piece(i + j)) return 0; // lower diagonal left
	}

	for (
		i = nextChar(piece_pos.charAt(0)), j = parseInt(piece_pos[1]) - 1;
		i < move.charAt(0) && j > move[1];
		i = nextChar(i), --j
	) {
		// letter + number
		// console.log(i,j)
		if (piece(i + j)) return 0; // lower diagonal left
	}

	return 1;
}

function diagonality(piece_pos, move) {
	var i, j;

	for (
		i = piece_pos.charAt(0), j = parseInt(piece_pos[1]);
		insideBoard(i + j);
		i = previousChar(i), ++j
	) {
		// letter + number
		if (i == move[0] && j == move[1]) return 1; // upper diagonal left
	}

	for (
		i = piece_pos.charAt(0), j = parseInt(piece_pos[1]);
		insideBoard(i + j);
		i = nextChar(i), ++j
	) {
		// letter + number
		if (i == move[0] && j == move[1]) return 1; // upper diagonal right
	}

	// lower diagonals
	for (
		i = piece_pos.charAt(0), j = parseInt(piece_pos[1]);
		insideBoard(i + j);
		i = previousChar(i), --j
	) {
		// letter + number
		if (i == move[0] && j == move[1]) return 1; // lower diagonal left
	}

	for (
		i = piece_pos.charAt(0), j = parseInt(piece_pos[1]);
		insideBoard(i + j);
		i = nextChar(i), --j
	) {
		// letter + number
		if (i == move[0] && j == move[1]) return 1; // lower diagonal right
	}

	return 0;
}

function moveKnight(color, move, takes) {
	var piece_name = (color == 1 ? "w" : "b") + "N";
	var piece_pos = findPiece(piece_name);
	var combination;
	for (var i in piece_pos) {
		if (canKnightMove(piece_pos[i], move)) {
			if (move.length == 3) move = move.slice(1);
			combination = piece_pos[i] + "-" + move;
			if (combination) return combination;
		}
	}
	// console.log(combination);
	return combination;
}

function canKnightMove(piece_pos, move) {
	var letter_piece = piece_pos.charAt(0);
	var number_piece = parseInt(piece_pos[1]);

	var letter_move = move.charAt(0);
	var number_move = parseInt(move[1]);

	var ambiguity = null; // if ambiguity, equals letter
	if (move.length == 3) {
		// ge7
		letter_move = move.charAt(1);
		number_move = parseInt(move[2]);
		ambiguity = move.charAt(0);
	}

	//top
	//top two left
	if (
		((letter_move == previousChar(letter_piece) &&
			number_move + 2 == number_piece) ||
			(letter_move == previousChar(previousChar(letter_piece)) &&
				number_move + 1 == number_piece)) &&
		insideBoard(move) &&
		(ambiguity == null || letter_piece == ambiguity)
	)
		return 1;
	//top two right
	if (
		((letter_move == nextChar(letter_piece) && number_move + 2 == number_piece) ||
			(letter_move == nextChar(nextChar(letter_piece)) &&
				number_move + 1 == number_piece)) &&
		insideBoard(move) &&
		(ambiguity == null || letter_piece == ambiguity)
	)
		return 1;

	//bottom
	//bottom two left
	if (
		((letter_move == previousChar(letter_piece) &&
			number_move - 2 == number_piece) ||
			(letter_move == previousChar(previousChar(letter_piece)) &&
				number_move - 1 == number_piece)) &&
		insideBoard(move) &&
		(ambiguity == null || letter_piece == ambiguity)
	)
		return 1;
	//bottom two right
	if (
		((letter_move == nextChar(letter_piece) && number_move - 2 == number_piece) ||
			(letter_move == nextChar(nextChar(letter_piece)) &&
				number_move - 1 == number_piece)) &&
		insideBoard(move) &&
		(ambiguity == null || letter_piece == ambiguity)
	)
		return 1;

	return 0;
}

function movePawn(color, move, takes) {
	// TODO prise en passant
	var combination;
	if (takes) {
		if (move.length == 2) {
			var column;

			// xbc5 written xc5
			column = color > 0 ? previousChar(move[0]) : nextChar(move[0]);
			var left_case = column + (parseInt(move[1]) - color); // = "b4"

			// xdc5
			column = color > 0 ? nextChar(move[0]) : previousChar(move[0]);
			var right_case = column + (parseInt(move[1]) - color); // = "d4"

			var left =
				piece(left_case) &&
				piece(left_case)[0] == (color == 1 ? "w" : "b") &&
				piece(left_case)[1] == "P";
			var right =
				piece(right_case) &&
				piece(right_case)[0] == (color == 1 ? "w" : "b") &&
				piece(right_case)[1] == "P";
			combination = (left ? left_case : right_case) + "-" + move[0] + move[1]; // d4-c5
		} else {
			// xdc5 from d4 to c5 with ambiguity on which pawn
			combination =
				move[0] + (parseInt(move[2]) - color) + "-" + move[1] + move[2]; // d4-c5
		}
	} else {
		// c5
		if (
			((color == 1 && move[1] == 4) || (color == -1 && move[1] == 5)) &&
			!piece(move[0] + (parseInt(move[1]) - color))
		) {
			combination =
				move[0] + (parseInt(move[1]) - 2 * color) + "-" + move[0] + move[1]; // e2-e4
		} else
			combination =
				move[0] + (parseInt(move[1]) - color) + "-" + move[0] + move[1]; // d4-d5
	}
	// console.log(combination);
	return combination;
}

function findPiece(piece_name) {
	return Object.keys(m_board.position).filter(
		(key) => m_board.position[key] === piece_name
	);
}

function piece(p) {
	return m_board.position[p];
}

function insideBoard(pos) {
	return !(
		pos.charAt(0) > "h" ||
		pos.charAt(0) < "a" ||
		pos[1] > 8 ||
		pos[1] < 1
	);
}

function nextChar(c) {
	return String.fromCharCode(c.charCodeAt(0) + 1);
}

function previousChar(c) {
	return String.fromCharCode(c.charCodeAt(0) - 1);
}
