from src.game.board import Board
import numpy as np
import matplotlib.pyplot as plt
import networkx as nx

class Node:
    def __init__(self, score, move, board):
        self.score = score
        self.move = move
        self.board = board
        self.children = []
    def add_child(self,child):
        self.children.append(child)

def get_leaves(node, leaves=None):
    if leaves is None:
        leaves = []

    if not node.children:
        leaves.append(node)
    else:
        for child in node.children:
            get_leaves(child, leaves)

    return leaves

def get_depth(node):
    if len(node.children) == 0:
        return 1
    return 1 + get_depth(node.children[0])

file_to_letter = {1:'A', 2:'B', 3:'C', 4:'D', 5:'E', 6:'F', 7:'G', 8:'H'}

def chess_notation(node, depth):
    if node.move == []:
        return str(node.score)
    board = node.board
    pos, move = node.move[0], node.move[1]
    piece_type =  board.get_piece_from_position(move).piece_type
    move_chess = file_to_letter[pos[0]]+str(pos[1])+file_to_letter[move[0]]+str(move[1])
    return f'{depth}.{piece_type+move_chess}: {node.score}'


#function to recursively create a graph from a tree
def add_nodes_edges(graph, node, depth):
    for child in node.children:
        graph.add_node(chess_notation(child,depth))
        graph.add_edge(chess_notation(node, depth-1), chess_notation(child,depth))
        add_nodes_edges(graph, child, depth+1)
#function to visualize the tree
def visualize_tree(root):
    graph = nx.Graph()
    graph.add_node(chess_notation(root,0))
    add_nodes_edges(graph, root, 1)

    # Manually assign positions based on depth
    depth_counts = {}
    for node in graph.nodes():
        depth = int(node.split('.')[0])
        if depth not in depth_counts:
            depth_counts[depth] = 0
        depth_counts[depth] += 1
    pos = {}
    for node in graph.nodes():
        depth = int(node.split('.')[0])
        node_index = list(graph.nodes()).index(node) % depth_counts[depth]
        pos[node] = (depth * 200, node_index * 100)

    plt.figure(figsize=(15, 10))

    # Draw nodes
    nx.draw_networkx_nodes(graph, pos, node_size=700)

    # Draw edges
    nx.draw_networkx_edges(graph, pos, edge_color='lightblue')

    # Draw labels
    labels = {node: node[2:] for node in graph.nodes()}
    nx.draw_networkx_labels(graph, pos, labels)

    plt.show()




class Tree:
    def __init__(self, score, move, board, eval_func):
        self.root = Node(score, move, board)
        self.leaves = []
        self.eval_func = eval_func

    def add_node(self, parent_node, node):
        parent_node.add_child(node)

    def trim_from_node(self,node):
        self.root = node
        self.leaves = get_leaves(node)
        assert self.leaves !=None


    def grow_leaves(self, depth):
        if depth == 0:
            return None

        if self.leaves == []:
            board = self.root.board
            if board.color_to_play=='w':
                best_score = float('-inf')
            else:
                best_score = float('inf')
            for piece,move in board.possible_moves(board.color_to_play):
                position_to_test = board.get_entire_position()
                Position_to_test = Board(0,position_to_test,moves=[],color_to_play=board.color_to_play)
                piece_pos_to_test = Position_to_test.get_piece_from_position([piece.file, piece.row])
                Position_to_test.Move(piece_pos_to_test,move)
                score_pos_test = self.eval_func(Position_to_test)

                # node = Node(score_pos_test, [[piece.file, piece.row],move], Position_to_test)
                # self.leaves.append(node)
                # self.add_node(self.root, node)

                if board.color_to_play == 'w':
                    if score_pos_test > best_score:
                        best_score = score_pos_test
                        boards_best_score = [Position_to_test]
                        move_best_score = [[[piece.file, piece.row], move]]
                    elif score_pos_test == best_score:
                        boards_best_score.append(Position_to_test)
                        move_best_score.append([[piece.file, piece.row], move])
                    else:
                        Position_to_test.delete_all_pieces()
                        del Position_to_test
                else:
                    if score_pos_test < best_score:
                        best_score = score_pos_test
                        best_score = score_pos_test
                        boards_best_score = [Position_to_test]
                        move_best_score = [[[piece.file, piece.row], move]]
                    elif score_pos_test == best_score:
                        boards_best_score.append(Position_to_test)
                        move_best_score.append([[piece.file, piece.row], move])
                    else:
                        Position_to_test.delete_all_pieces()
                        del Position_to_test
            for move, board in zip(move_best_score, boards_best_score):
                node = Node(best_score, move, board)
                self.leaves.append(node)
                self.add_node(self.root, node)
            self.grow_leaves(depth-1)

        else:
            leaves = []
            for node_leaf in self.leaves:
                board = node_leaf.board
                move_best_score, boards_best_score = [], []
                if board.color_to_play=='w':
                    best_score = float('-inf')
                else:
                    best_score = float('inf')
                for piece,move in board.possible_moves(board.color_to_play):
                    position_to_test = board.get_entire_position()
                    Position_to_test = Board(0,position_to_test,moves=[],color_to_play=board.color_to_play)
                    piece_pos_to_test = Position_to_test.get_piece_from_position([piece.file, piece.row])
                    Position_to_test.Move(piece_pos_to_test,move)
                    score_pos_test = self.eval_func(Position_to_test)

                    # node = Node(score_pos_test, [[piece.file, piece.row],move], Position_to_test)
                    # self.leaves.append(node)
                    # self.add_node(self.root, node)

                    if board.color_to_play == 'w':
                        if score_pos_test > best_score:
                            best_score = score_pos_test
                            boards_best_score = [Position_to_test]
                            move_best_score = [[[piece.file,piece.row],move]]
                        elif score_pos_test == best_score:
                            boards_best_score.append(Position_to_test)
                            move_best_score.append([[piece.file,piece.row],move])
                        else:
                            Position_to_test.delete_all_pieces()
                            del Position_to_test
                    else:
                        if score_pos_test < best_score:
                            best_score = score_pos_test
                            best_score = score_pos_test
                            boards_best_score = [Position_to_test]
                            move_best_score = [[[piece.file,piece.row],move]]
                        elif score_pos_test == best_score:
                            boards_best_score.append(Position_to_test)
                            move_best_score.append([[piece.file,piece.row],move])
                        else:
                            Position_to_test.delete_all_pieces()
                            del Position_to_test
                for move, board in zip(move_best_score, boards_best_score):
                    node = Node(best_score, move, board)
                    leaves.append(node)
                    self.add_node(node_leaf, node)
            self.leaves = leaves
            self.grow_leaves(depth-1)

def min_max(node, depth, maximizing_player):
    if depth == 0 or len(node.children)==0:
        return node.score
    if maximizing_player:
        max_eval = float('-inf')
        for child in node.children:
            eval = min_max(child, depth-1, False)
            max_eval = max(max_eval, eval)
        return max_eval
    else:
        min_eval = float('inf')
        for child in node.children:
            eval = min_max(child, depth-1, True)
            min_eval = min(min_eval, eval)
        return min_eval


def evaluate_pos_simplest(board):
    """returns score for the position only based on pieces value"""
    piece_value = {'Q': 9, 'r': 5, 'b': 3, 'k': 3, 'p': 1, 'K':0}
    if board.result == 'w':
        return float('inf')
    elif board.result == 'b':
        return float('-inf')
    elif board.result == 'd':
        return 0
    else:
        score = 0
        for piece in board.pieces['w']:
            score += piece_value[piece.piece_type]
        for piece in board.pieces['b']:
            score -= piece_value[piece.piece_type]
        return score

class Player_Min_Max:
    def __init__(self,color,board,depth,eval_func = evaluate_pos_simplest):
        self.eval_func = eval_func
        self.board = board
        self.color = color
        self.depth = depth
        self.tree = Tree(0, [], board, eval_func)
        self.tree.grow_leaves(depth)


    def Update_tree_on_opponent_move(self, move): #[[file_start, row_start], [file_end, row_end]]

        move_in_tree = False
        for child in ((self.tree).root).children:
            if child.move == move:
                move_in_tree = True
                (self.tree).trim_from_node(child)
                (self.tree).grow_leaves(1)
                break
        if not move_in_tree:
            self.tree = Tree(0, [], self.board, self.eval_func)
            self.tree.grow_leaves(self.depth)
        visualize_tree(self.tree.root)

    def self_move(self):
        best_score = min_max(self.tree.root, self.depth, self.color=='w')
        moves = []
        nodes_moves = []
        print(best_score)
        for child in self.tree.root.children:
            leaves_child = get_leaves(child)
            for leaf in leaves_child:
                # print(f'move: {child.move}, move_score: {child.score}')
                # print(f'leaf_move: {leaf.move}, leafscore: {leaf.score}')
                # print(' ')
                if leaf.score == best_score:
                    moves.append(child.move)
                    nodes_moves.append(child)
        assert len(moves)!=0
        rand_index = np.random.randint(len(moves))
        move = moves[rand_index]
        piece_to_move = self.board.get_piece_from_position(move[0])
        self.board.Move(piece_to_move, move[1])
        (self.tree).trim_from_node(nodes_moves[rand_index])
        (self.tree).grow_leaves(1)
