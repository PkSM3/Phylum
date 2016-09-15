


import networkx as nx
from collections import defaultdict
import time


class CPM:

	def __init__(self , G ):
		self.G = G
		self.all_cliques = []

	def find_cliques ( self ):
		start = time.time()

		self.all_cliques = [c for c in nx.find_cliques( self.G ) if len(c) >= 3]
		
		end = time.time()
		CPM_cliques_time = float("{0:.5f}".format((end - start)))
		return CPM_cliques_time

	def get_percolated_cliques(self , k ):
		
		G = self.G
		all_cliques = self.all_cliques

		perc_graph = nx.Graph()
		if all_cliques == False:
			cliques = [frozenset(c) for c in nx.find_cliques(G) if len(c) >= k]
		else:
			cliques = [frozenset(c) for c in all_cliques if len(c) >= k]
		perc_graph.add_nodes_from(cliques)

		# First index which nodes are in which cliques
		membership_dict = defaultdict(list)
		for clique in cliques:
			for node in clique:
				membership_dict[node].append(clique)

		# For each clique, see which adjacent cliques percolate
		for clique in cliques:
			for adj_clique in self.get_adjacent_cliques(clique, membership_dict):
				if len(clique.intersection(adj_clique)) >= (k - 1):
					perc_graph.add_edge(clique, adj_clique)

		# Connected components of clique graph with perc edges
		# are the percolated cliques
		for component in nx.connected_components(perc_graph):
			yield(frozenset.union(*component))

	def get_adjacent_cliques(self , clique, membership_dict):
		adjacent_cliques = set()
		for n in clique:
			for adj_clique in membership_dict[n]:
				if clique != adj_clique:
					adjacent_cliques.add(adj_clique)
		return adjacent_cliques

